#!/bin/csh -f

foreach file (*.script)
  echo "--- $file ---"

  CDoc $file
end
