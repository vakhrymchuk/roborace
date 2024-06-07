WEB_INPUT=web
WEB_OUTPUT=web-out
SRC_WEB_H=src/web.h

html-compress:
	htmlcompressor ${WEB_INPUT}/index.html > ${WEB_OUTPUT}/incex.html --remove-intertag-spaces --compress-js --compress-css
	htmlcompressor ${WEB_INPUT}/script.js > ${WEB_OUTPUT}/script.js --remove-intertag-spaces --compress-js --compress-css
	htmlcompressor ${WEB_INPUT}/style.css > ${WEB_OUTPUT}/style.css --remove-intertag-spaces --compress-js --compress-css
	

html2h: html-compress
	./html2h.py ${WEB_OUTPUT} > ${SRC_WEB_H}

html2h-non-minimized:
	./html2h.py ${WEB_INPUT} > ${SRC_WEB_H}

html2h-nostrip:
	./html2h.py ${WEB_INPUT} --nostrip > ${SRC_WEB_H}

pio-run: html2h
	pio run
