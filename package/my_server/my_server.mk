################################################################################
#
# embeddedinn package
#
################################################################################

MYSERVER_VERSION = 1.0
MYSERVER_SITE = package/my_server/src
MYSERVER_SITE_METHOD = local# Other methods like git,wget,scp,file etc. are also available.

define MYSERVER_BUILD_CMDS
    $(MAKE) CC="$(TARGET_CC)" LD="$(TARGET_LD)" -C $(@D)
endef

define MYSERVER_INSTALL_TARGET_CMDS
    $(INSTALL) -D -m 0755 $(@D)/my_server  $(TARGET_DIR)/usr/bin
endef

$(eval $(generic-package))
