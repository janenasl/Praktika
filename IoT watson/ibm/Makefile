include $(TOPDIR)/rules.mk

PKG_NAME:=ibm
PKG_RELEASE:=1
PKG_VERSION:=1.0.0

include $(INCLUDE_DIR)/package.mk

define Package/ibm
	CATEGORY:=Base system
	TITLE:=IBM
	DEPENDS:=+libiotp_device +libubus +libubox +libblobmsg-json +libuci
endef

define Package/ibm/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ibm $(1)/usr/bin
	$(INSTALL_BIN) ./files/ibm.init $(1)/etc/init.d/ibm
	$(INSTALL_CONF) ./files/ibm.config $(1)/etc/config/ibm
endef

$(eval $(call BuildPackage,ibm))
