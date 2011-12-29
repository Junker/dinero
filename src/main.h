/*
 * main.h
 *
 * Copyright (C) 2011 - root
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <libgda-ui/libgda-ui.h>



gchar *home;

enum
{
	OPER_INSERT,
	OPER_UPDATE
};

#define OperType gint

//Lookup Models
GdaDataModel *account_model,
  			 *category_model,
             *out_category_model,
             *in_category_model,
             *subcategory_model,
             *unit_model,
             *currency_model,
             *debtor_model,
             *creditor_model,
			 *person_model,
             *periodicity_model;


void refresh_account_model();
void refresh_currency_model();
void refresh_category_model();
void refresh_in_category_model();
void refresh_out_category_model();
void refresh_subcategory_model();
void refresh_unit_model();
void refresh_person_model();
