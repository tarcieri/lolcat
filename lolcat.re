#
# lolcat.re: irccat for Reia
# Copyright (C)2010 Tony Arcieri
# 
# Redistribution is permitted under the MIT license.  See LICENSE for details.
#

class Lolcat
  def initialize
    nick = "powcat"
    
    @sock = TCPSocket("irc.freenode.net", 6667, {:packet => :line})
    @sock.write("USER #{nick} * * #{nick}\n")
    @sock.write("NICK #{nick}\n")
  end
  
  def run
    @sock.read().to_s().print()
    run()
  end
end

Lolcat().run()