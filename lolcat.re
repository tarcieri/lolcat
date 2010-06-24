#
# lolcat.re: irccat for Reia
# Copyright (C)2010 Tony Arcieri
# 
# Redistribution is permitted under the MIT license.  See LICENSE for details.
#

class Lolcat
  def initialize(server, port, nick, channel)
    (@server, @port, @nick, @channel) = (server, port, nick, channel)    
    @sock = TCPSocket(@server, @port, {:packet => :line})
  end
  
  def register
    @sock.write("USER #{@nick} * * #{@nick}\n")
    @sock.write("NICK #{@nick}\n")
    #@sock.write("JOIN #{@channel}\n")
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
    router = Router(line)
    
    if router.ping?()
      handle_ping(line)
    end
  end
  
  def handle_ping(line)
    [_, message] = %r/PING :(.*?)[\r\n]*$/.match(line)
    Main.puts("OHAI THAR SERVAR #{message}! PONG!!!")
    @sock.write("PONG :#{message}\r\n")
  end
end

class Router
  def initialize(line)
    @line = line
  end
  
  def ping?
    %r/^PING /.match(@line)
  end
end

cat = Lolcat("irc.freenode.net", 6667, "powcat", "#reia")
cat.register()
cat.run()