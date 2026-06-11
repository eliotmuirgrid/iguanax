MEMBERS_BASE_CSS=\
jquery-ui-1.10.3.custom.css\
datatables/jquery.dataTables.css\
datatables/TableTools.css\
help_popup/help_popup.css\

MEMBERS_GENERAL_CSS=\
licensing.css\

MEMBERS_IGUANA_CSS=\
iguana.css\

MEMBERS_JS=\
jquery-1.10.2.js\
jquery-ui-1.10.3.custom.js\
purl.js\
datatables/jquery.dataTables.js\
datatables/TableTools.js\
datatables/ZeroClipboard.js\
jquery.formatCurrency-1.4.0.js\
license_comment.js\
ifware.js\
local_storage.js\
license_config.js\
window.js\
help_popup/help_popup.js\
licensing.js\
pagehandlers.js\
page.js\

ifneq (,$(JSZ_JAVA_HOME))
JSZ_JAVA=$(JSZ_JAVA_HOME)/bin/java
else
JSZ_JAVA=java
endif

all: members_base.css members_general.css members_iguana.css members.js

clean:: 
	rm ../members_base.css ../members_general.css ../members_iguana.css ../members.js

members_base.css:
	cat $(foreach file,$(MEMBERS_BASE_CSS),$(file)) | java -jar ../../../JSZ/yuicompressor/yuicompressor-2.4.7.jar --type css -o "../members_base.css"

members_general.css:
	cat $(foreach file,$(MEMBERS_GENERAL_CSS),$(file)) | java -jar ../../../JSZ/yuicompressor/yuicompressor-2.4.7.jar --type css -o "../members_general.css"

members_iguana.css:
	cat $(foreach file,$(MEMBERS_IGUANA_CSS),$(file)) | java -jar ../../../JSZ/yuicompressor/yuicompressor-2.4.7.jar --type css -o "../members_iguana.css"

members.js:
	cat $(foreach file,$(MEMBERS_JS),$(file)) | $(JSZ_JAVA) -jar ../../../JSZ/closure/compiler.jar --js_output_file "../members.js" --warning_level QUIET
