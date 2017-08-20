#!/bin/sh
#
# Copyright 2007,2009-2010,2013 Viveris Technologies
# Copyright 2011-2014 Didier Barvaux
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
#

# Script to generate all required files for `configure' when
# starting from a fresh repository checkout.

run()
{
  binary_name="$1"
  shift
  args="$@"

  echo -n "Running ${binary_name}... "

  binary_path=$( which ${binary_name} 2>/dev/null )
  if [ -z "$binary_path" ] || [ ! -x "$binary_path" ] ; then
    echo "failed"
    echo "Command ${binary_name} not found, please install it"
    exit 1
  fi

  $binary_path $args >/dev/null 2>&1
  if [ $? -eq 0 ] ; then
    echo "done"
  else
    echo "failed"
    echo "Running ${binary_name} again with errors unmasked:"
    $binary_path $args
    exit 1
  fi
}

rm -f config.cache
rm -f config.log

OLD_PWD="$PWD"
cd $( dirname $0 ) &>/dev/null

run aclocal
run libtoolize --force
run autoconf
run autoheader
run automake --add-missing

cd ${OLD_PWD} &>/dev/null

# run configure with failure on compiler warnings enabled
chmod +x $( dirname $0 )/configure
$( dirname $0 )/configure \
	--enable-rohc-debug \
	--enable-fail-on-warning \
	--enable-fortify-sources \
	$@

