CODE_DIR = halang

main: 
	$(MAKE) -C $(CODE_DIR)

test:
	$(MAKE) -C $(CODE_DIR) test

clean:
	$(MAKE) -C $(CODE_DIR) clean
