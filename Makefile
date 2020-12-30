default: all

.DEFAULT:
	$(MAKE) -C src $@

webapp:
	cd webapp && npm install && npm run build

.PHONY: webapp