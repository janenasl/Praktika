map = Map("ibm")

section = map:section(NamedSection, "ibm_sct", "ibm", "IBM section")

flag = section:option(Flag, "enable", "Enable", "Enable program")

orgId = section:option(Value, "orgId", "Organization id")
orgId.maxlength = 100
orgId.required

typeId = section:option(Value, "typeId", "Type id")
typeId.maxlength = 100
typeId.required

deviceId = section:option(Value, "deviceId", "Device id")
deviceId.maxlength = 100
deviceId.required

auth = section:option(Value, "auth", "Authentification token")
auth.datatype = "string"
auth.maxlength = 100
auth.required

return map
