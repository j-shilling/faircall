/*
 *    This file is part of faircall.
 *
 *    faircall is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    faircall is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with faircall.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __ERROR_H__
#define __ERROR_H__

/* Error codes used with GError */
enum
{
  NO_NAME_ERROR,
  NAME_NOT_UNIQUE_ERROR,
  XML_PARSE_ERROR,
  NO_OPEN_CLASS_ERROR,
  NOT_ENOUGH_STUDENTS_ERROR,
  NO_SUCH_NAME_ERROR,

  UNKOWN_ERROR

} FaircallError;



#endif /* __ERROR_H__ */
