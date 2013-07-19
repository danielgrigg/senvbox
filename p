#!/bin/bash

# Manually generate a preview
sandbox-exec -f /usr/share/sandbox/quicklookd.sb /usr/bin/qlmanage -p -c com.sliplanesoftware.senv -g ~/Library/QuickLook/senvql.qlgenerator $1 > /dev/null
