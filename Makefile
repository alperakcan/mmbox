all:
	@make -C src
	@echo "Done ..."

install:
	@cp -R share /usr/local/share/tetromino
	@cp tetromino /usr/local/bin/
	@echo "Done ..."
	@echo "Now, just type tetromino"

uninstall:
	@rm -rf /usr/local/share/tetromino
	@rm /usr/local/bin/tetromino
	@echo "Done ..."

clean:
	@make clean -C src
	@echo "Cleaned ..."
