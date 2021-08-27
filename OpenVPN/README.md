**controll your openvpn server through ubus!**

show existing ubus methods: ubus -v list telnet

**method:** ubus call telnet log '{"value":integer}' <br>
      value 0 - show all messages <br>
      value **n** - show the most recent **n** lines of log file history.
      
**method:** ubus call telnet hold '{"hold":"string"}' <br>
      hold - show current state of hold <br>
      hold on/off - turn on/off openvpn hold <br>
      hold release - leave hold state and start OpenVPN, but do not alter the current hold flag setting. <br>
      
**method:** ubus call telnet mute '{"mute":"string"}' <br>
      mute - show the current verb setting <br> 
      mute **n** -  change the verb parameter to **n** <br>
      
**method:** ubus call telnet status <br>
      get information about connected clients <br>
      
**method:** ubus call telnet pid <br>
      get the process ID of the current OpenVPN process. <br>
