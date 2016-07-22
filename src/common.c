
/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * common.c
 * Copyright (C) Dmitry Kosenkov <junker@front.ru>
 * 
 * main.c is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * main.c is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <glib.h>
#include "common.h"
#include "db.h"

void refresh_account_model()
{
	if (G_IS_OBJECT(account_model)) g_object_unref(account_model);
	account_model = db_exec_select_sql ("SELECT id,name FROM account");
}

void refresh_currency_model()
{
	if (G_IS_OBJECT(currency_model)) g_object_unref(currency_model);
	currency_model = db_exec_select_sql ("SELECT id,name FROM currency");
}

void refresh_category_model()
{
	if (G_IS_OBJECT(in_category_model)) g_object_unref(category_model);
	category_model = db_exec_select_sql ("SELECT id,name FROM category");
}

void refresh_in_category_model()
{
	if (G_IS_OBJECT(in_category_model)) g_object_unref(in_category_model);
	in_category_model = db_exec_select_sql ("SELECT id,name FROM category WHERE type=2");
}

void refresh_out_category_model()
{
	if (G_IS_OBJECT(out_category_model)) g_object_unref(out_category_model);
	out_category_model = db_exec_select_sql ("SELECT id,name FROM category WHERE type=1");
}

void refresh_subcategory_model()
{
	if (G_IS_OBJECT(subcategory_model)) g_object_unref(subcategory_model);
	subcategory_model = db_exec_select_sql ("SELECT id,name FROM subcategory");
}

void refresh_unit_model()
{
	if (G_IS_OBJECT(unit_model)) g_object_unref(unit_model);
	unit_model = db_exec_select_sql ("SELECT id,name FROM unit");
}

void refresh_person_model()
{
	if (G_IS_OBJECT(person_model)) g_object_unref(person_model);
	person_model = db_exec_select_sql ("SELECT id,name FROM person");
}