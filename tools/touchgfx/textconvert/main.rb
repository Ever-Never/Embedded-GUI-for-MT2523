###############################################################################
#
# @brief     This file is part of the TouchGFX 4.5.0 evaluation distribution.
#
# @author    Draupner Graphics A/S <http://www.touchgfx.com>
#
###############################################################################
#
# @section Copyright
#
# Copyright (C) 2014-2016 Draupner Graphics A/S <http://www.touchgfx.com>.
# All rights reserved.
#
# TouchGFX is protected by international copyright laws and the knowledge of
# this source code may not be used to write a similar product. This file may
# only be used in accordance with a license and should not be re-
# distributed in any way without the prior permission of Draupner Graphics.
#
# This is licensed software for evaluation use, any use must strictly comply
# with the evaluation license agreement provided with delivery of the
# TouchGFX software.
#
# The evaluation license agreement can be seen on www.touchgfx.com
#
# @section Disclaimer
#
# DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Draupner Graphics A/S has
# no obligation to support this software. Draupner Graphics A/S is providing
# the software "AS IS", with no express or implied warranties of any kind,
# including, but not limited to, any implied warranties of merchantability
# or fitness for any particular purpose or warranties against infringement
# of any proprietary rights of a third party.
#
# Draupner Graphics A/S can not be held liable for any consequential,
# incidental, or special damages, or any other relief, or for any claim by
# any third party, arising from your use of this software.
#
###############################################################################
$:.unshift File.dirname(__FILE__)
require 'rubygems'
require 'lib/generator'
require 'lib/emitters/fonts_cpp'

WINDOWS_LINE_ENDINGS = "\r\n"
UNIX_LINE_ENDINGS = "\n"
#on windows/mingw file.write will it self translate \n to \r\n, on linux not
LINE_ENDINGS = RUBY_PLATFORM.match(/linux/) ? WINDOWS_LINE_ENDINGS : UNIX_LINE_ENDINGS

class Main
    def self.banner
        <<-BANNER
Create cpp text files from excel translations
              
Usage: #{File.basename($0)} file.xlsx path/to/fontconvert.out path/to/fonts_output_dir path/to/localization_output_dir path/to/font/asset
BANNER
    end

    if __FILE__ == $0
        if ARGV.count < 6
            puts self.banner
            exit
        end      
        file_name = ARGV.shift
        FontsCpp.font_convert = ARGV.shift
        fonts_output_path = ARGV.shift
        localization_output_path = ARGV.shift
        font_asset_path = ARGV.shift
        $calling_path = ARGV.shift
        data_format = ARGV.shift
        begin
            Generator.new.run(file_name, fonts_output_path, localization_output_path, font_asset_path, data_format)
        rescue Exception => e
            # Makefiles depend on this file, so remove in case of error.
            FileUtils.rm_f "#{localization_output_path}/include/texts/TextKeysAndLanguages.hpp"
            abort("an error occurred in converting texts:\n#{e}")
        end
    end
end
