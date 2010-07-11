#
# lolcat.re: irccat for Reia
# Copyright (C)2010 Tony Arcieri
# 
# Redistribution is permitted under the MIT license.  See LICENSE for details.
#

host    = "irc.freenode.net"
port    = 6667
nick    = "lolcat"
channel = "#reia"

listen_addr = "0.0.0.0"
listen_port = 3210

class Lolcat
  def initialize(@server, @port, @nick, @channel)
    @sock = TCPSocket(@server, @port, {:packet => :line})
  end
  
  # attr_reader ftw
  def server; @server; end
  def port; @port; end
  def nick; @nick; end
  def channel; @channel; end
  
  def register
    @sock.write("USER #{@nick} * * #{@nick}\n")
    @sock.write("NICK #{@nick}\n")
    @sock.write("JOIN #{@channel}\n")
  end
  
  def run
    line = read_line()    
    process_line(line)
    run()
  end
  
  def read_line
    @sock.read().to_s().chop()
  end
  
  def process_line(line)
    line.puts()    
    
    if %r/^PING /.match(line)
      handle_ping(line)
    elseif %r/PRIVMSG (#[a-z0-0]+) :ohai/.match(line)
      send_message("PRIVMSG #{@channel} :OHAI!!")
    end
  end
  
  def handle_ping(line)
    # The colors, Duke! The colors!
    [_, message] = %r/PING :(.*?)[\r\n]*$/.match(line)
    
    # Lulzy debug message
    Main.puts("OHAI THAR SERVAR #{message}! PONG!!!")
    
    # Oh yeah here's where we actually do the important thing
    send_message("PONG :#{message}")
  end
  
  def send_message(msg)
    @sock.write("#{msg}\r\n")
  end
end

class Server
  def initialize(@lolcat, @addr, @port)
    @server = TCPServer(@addr, @port, {:packet => :line})
    Main.puts("*** TCP server listening on #{@addr}:#{@port}")
  end
  
  def run
    connection = ConnectionHandler(@lolcat, @server.accept())
    Process.spawn { connection.run() }
    run()
  end
end

class ConnectionHandler
  def initialize(@lolcat, @sock); end
  
  def run()
    line = @sock.read()
    if line
      @lolcat ! (:line, line)
      run()
    end
  end
end

cat = Lolcat(host, port, nick, channel)
cat.register()

Process.spawn_link { Server(cat, listen_addr, listen_port).run() }

cat.run()