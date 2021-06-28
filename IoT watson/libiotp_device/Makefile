include $(TOPDIR)/rules.mk

PKG_NAME:=libiotp_device
PKG_RELEASE:=1
PKG_VERSION:=1.0.0
PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/ibm-watson-iot/iot-c
PKG_SOURCE_VERSION:=afec329eda0caa917a0f4fd5262de17aa3f00996

include $(INCLUDE_DIR)/package.mk
	
define Package/libiotp_device
	CATEGORY:=Libraries
	TITLE:=libiotp_device
	DEPENDS:=+libopenssl
endef

define Package/libiotp_device/description
	This is a internet of things devices library
endef

define Package/libiotp_device/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/build/*.so $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/paho.mqtt.c/build/output/*.so* $(1)/usr/lib
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/lib $(1)/usr/include
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/paho.mqtt.c/build/output/*.so* $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/build/*.so $(1)/usr/lib/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/src/wiotp/sdk/*.h $(1)/usr/include/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/paho.mqtt.c/src/*.h $(1)/usr/include/
endef

$(eval $(call BuildPackage,libiotp_device))
