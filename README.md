Working on reforming and improving the Open Tibia community using democratic voting, open source software and software models (replicable data), and general optimism and support of the different Open Tibia communities and members.
Join us at https://opentibia.dev/ and click the discord icon.

### Otclient with support for loading of encrypted zipped data/modules

Otclient can now load data/modules/init.lua from data.zip and modules.zip by setting the preprocessor ZIPENCRYPTION.
The zips must be encrypted with ZipCrypto, not AES.
Default password m_zipPassword = "12345678" is set in src/framework/core/resourcemanager.h.
Init.lua must be included in either data.zip or modules.zip.

To create encrypted zips:

*    Install 7-zip
*    Copy init.lua into data
*    Go into data
*    Select all, right click, 7-zip, Add to archive...
*    Set password and ZipCrypto as encryption method
*    Repeat for modules directory

Credits to Zelek for most of this (I tweaked/fixed it a little). Paid him $18 for this in mid-late 2020.
