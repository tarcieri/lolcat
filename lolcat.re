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
    router = Router(self, line)
    
    if router.ping?()
      handle_ping(line)
    elseif router.ohai?()
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

class Router
  def initialize(bot, line)
    (@bot, @line) = (bot, line)
  end
  
  def ping?
    %r/^PING /.match(@line)
  end
  
  def ohai?
    case %r/PRIVMSG (#[a-z0-0]+) :ohai/.match(@line)
    when [_, channel]
      channel == @bot.channel()
    end
  end
end

cat = Lolcat("irc.freenode.net", 6667, "ceilingcat", "#reia")
cat.register()
cat.run()