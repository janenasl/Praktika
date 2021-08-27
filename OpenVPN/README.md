**controll your openvpn server through ubus!**

all information about openVPN management-interface commands: https://openvpn.net/community-resources/management-interface/

**1. openVPN management-interface wont work on openWRT if openVPN source is build with disabled OPENVPN_openssl_ENABLE_MANAGEMENT** <br><br>
**2.pkcs methods wont work on openWRT if openvpn source is build with disabled config OPENVPN_openssl_ENABLE_PKCS11**


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
      
**method:** ubus call telnet auth '{"auth":"string"}' <br>
      auth - show the current auth setting <br> 
      auth interact -  change the auth parameter to interactive <br>
      auth non-interact -  change the auth parameter to non-interactive <br>
      
**method:** ubus call telnet kill '{"kill":"string"}' <br>
      kill common-name/source addres:port - kill a particlar client instance. <br> 
      
**method:** ubus call telnet signal '{"signal":"string"}' <br>
      signal **signal name** - set signal <br> 

**method:** ubus call telnet state '{"state":"Integer"}' <br>
      state - show the current verb setting <br> 
      state 0 - show all state transitions <br>
      state **n** - show the 3 most recent state transitions. <br>
      
**method:** ubus call telnet verb '{"verb":"integer"}' <br>
      mute - show the current verb setting <br> 
      verb **n** -  change the verb parameter to **n** <br>
      
**method:** ubus call telnet pkcs_index '{"index":"Integer"}' <br>
      index **n** - retrieve certificate by index **n** <br>
      
**method:** ubus call telnet status <br>
      get information about connected clients <br>
      
**method:** ubus call telnet pid <br>
      get the process ID of the current OpenVPN process. <br>
      
**method:** ubus call telnet pkcs <br>
      retrieve available number of certificates. <br>
      
**method:** ubus call telnet version <br>
      get version of current openVPN process <br>
