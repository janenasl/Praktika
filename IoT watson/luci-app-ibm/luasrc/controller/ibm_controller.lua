module("luci.controller.ibm_controller", package.seeall)

function index()
	entry({"admin", "services", "ibm"}, cbi("ibm_model"), _("IBM connector"),110)
end
