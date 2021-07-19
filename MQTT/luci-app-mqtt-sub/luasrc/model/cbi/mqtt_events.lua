require("uci")
m = Map("mqtt_subs")


s = m:section(TypedSection, "mqtt_event", translate("MQTT Events"))
s.addremove = true
s.anonymous = true
s.template = "cbi/tblsection"
s.novaluetext = translate("There are no events created yet.")

topic_name = s:option(ListValue, "topic", translate("Topic"), translate("MQTT subscriber topic names"));
m.uci:foreach("mqtt_subs","mqtt_topic",
    function(i)
        topic_name:value(i.topic, i.topic)
    end)

type = s:option(ListValue, "type", translate("event type"), translate("Choose event type"))
type:value("Decimal", translate("Decimal"))
type:value("String", translate("String"))

value = s:option(Value, "value", translate("value of event"), translate("Enter value of event in JSON format"))
value.rmempty = false

decval = s:option(ListValue, "dec_operators", translate("Decimal Operators"), translate("Select decimal operator"))
decval:depends("type","Decimal")
decval:value("0", "<")
decval:value("1", ">")
decval:value("2", "<=")
decval:value("3", ">=")
decval:value("4", "==")
decval:value("5", "!=")

strval = s:option(ListValue, "str_operator", translate("String Operators"), translate("Select string operator"))
strval:depends("type","String")
strval:value("0", "Equal To")
strval:value("1", "Not Equal To")

email = s:option(Value, "user_email", translate("Email address of user"), translate("Email address of user which will be informed in case of event success"))
email.datatype = "email"
email.rmempty = false
email.maxlength = "100"


return m
