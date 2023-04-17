drv_SUBS := dma intc mmc net serial spi timer video
$(eval $(call MODULE_template,drv))

drv: dma intc mmc net serial spi timer video
$(foreach module,$(drv_SUBS),$(eval include $(drv_SDIR)$(module)/Makefile))