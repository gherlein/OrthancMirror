/**
 * Orthanc - A Lightweight, RESTful DICOM Store
 * Copyright (C) 2012-2016 Sebastien Jodogne, Medical Physics
 * Department, University Hospital of Liege, Belgium
 * Copyright (C) 2017-2020 Osimis S.A., Belgium
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * In addition, as a special exception, the copyright holders of this
 * program give permission to link the code of its release with the
 * OpenSSL project's "OpenSSL" library (or with modified versions of it
 * that use the same license as the "OpenSSL" library), and distribute
 * the linked executables. You must obey the GNU General Public License
 * in all respects for all of the code used other than "OpenSSL". If you
 * modify file(s) with this exception, you may extend this exception to
 * your version of the file(s), but you are not obligated to do so. If
 * you do not wish to do so, delete this exception statement from your
 * version. If you delete this exception statement from all source files
 * in the program, then also delete it here.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 **/


#include "DicomPath.h"

#include "OrthancPluginException.h"

#include <boost/lexical_cast.hpp>

namespace OrthancPlugins
{
  const DicomPath::Prefix& DicomPath::GetPrefixItem(size_t depth) const
  {
    if (depth >= prefix_.size())
    {
      ORTHANC_PLUGINS_THROW_EXCEPTION(ParameterOutOfRange);
    }
    else
    {
      return prefix_[depth];
    }
  }


  DicomPath::Prefix& DicomPath::GetPrefixItem(size_t depth)
  {
    if (depth >= prefix_.size())
    {
      ORTHANC_PLUGINS_THROW_EXCEPTION(ParameterOutOfRange);
    }
    else
    {
      return prefix_[depth];
    }
  }


  DicomPath::DicomPath(const DicomTag& sequence,
                       size_t index,
                       const DicomTag& tag) :
    finalTag_(tag)
  {
    AddToPrefix(sequence, index);
  }


  DicomPath::DicomPath(const DicomTag& sequence1,
                       size_t index1,
                       const DicomTag& sequence2,
                       size_t index2,
                       const DicomTag& tag) :
    finalTag_(tag)
  {
    AddToPrefix(sequence1, index1);
    AddToPrefix(sequence2, index2);
  }


  DicomPath::DicomPath(const DicomTag& sequence1,
                       size_t index1,
                       const DicomTag& sequence2,
                       size_t index2,
                       const DicomTag& sequence3,
                       size_t index3,
                       const DicomTag& tag) :
    finalTag_(tag)
  {
    AddToPrefix(sequence1, index1);
    AddToPrefix(sequence2, index2);
    AddToPrefix(sequence3, index3);
  }


  std::string DicomPath::Format() const
  {
    std::string s;
      
    for (size_t i = 0; i < GetPrefixLength(); i++)
    {
      s += (GetPrefixTag(i).FormatHexadecimal() + " / " +
            boost::lexical_cast<std::string>(i) + " / ");
    }

    return s + GetFinalTag().FormatHexadecimal();
  }
}
