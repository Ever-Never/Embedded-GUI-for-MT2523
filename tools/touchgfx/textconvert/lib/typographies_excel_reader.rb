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
require 'lib/excel_reader'
require 'lib/typographies'

class TypographiesExcelReader
  attr_reader :reader

  def initialize(file_name)
    header_row_number = 3
    header_column_number = 2
    @reader = ExcelReader.new(file_name, "Typography", header_row_number, header_column_number)
    @typographies = []
  end

  def run
    reader.read_rows do |row|
      name = row[:"Typography Name"]
      font = row[:font]
      size = row[:size]
      bpp = row[:bpp]

      if name then name = name.strip end
      if font then font = font.strip end
      if size then size = size.strip end
      if bpp then bpp = bpp.strip end

      if name && font && size && bpp
        @typographies.push Typography.new(name, font, size.to_i, bpp.to_i)
      end
    end
    @typographies
  end

end
