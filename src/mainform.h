/*
 * Copyright (C) Dmitry Kosenkov 2009 <junker@front.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301,  USA
 */
 
#include <gtk/gtk.h>

#include "ex-grid.h"


//Expenditure/Income grid columns
enum  {
	EX_ID_COL,
	EX_TIME_T_COL,
	EX_DATE_COL,
	EX_ACCOUNT_ID_COL,
	EX_CATEGORY_ID_COL,
	EX_SUBCATEGORY_ID_COL,
	EX_QUANTITY_COL,
	EX_UNIT_ID_COL,
	EX_AMOUNT_COL,
	EX_CURRENCY_ID_COL,
	EX_DESCR_COL
};

// Account short grid columns
enum  {
	ACS_ID_COL,
	ACS_NAME_COL,
	ACS_STARTUP_ID_COL,
	ACS_STARTUP_AMOUNT_COL,
	ACS_EXPEND_COL,
	ACS_INCOME_COL,
	ACS_AMOUNT_COL,
	ACS_CURRENCY_ID_COL,
	ACS_DESCR_COL
};

//Account full grid columns
enum  {
	ACF_TIME_T_COL,	
	ACF_DATE_COL,
	ACF_ID_COL,
	ACF_AMOUNT_EXPEND_COL,
	ACF_AMOUNT_INCOME_COL,
	ACF_AMOUNT_DIF_COL,
	ACF_CURRENCY_ID_COL
};

//Debt/Credit grid columns
enum {
	DEB_ID_COL,
	DEB_TIME_T_COL,
	DEB_DATE_COL,
	DEB_ACCOUNT_ID_COL,
	DEB_PERSON_ID_COL,
	DEB_PERCENT_COL,
	DEB_PERIOD_COL,
	DEB_IS_CLOSED_COL,
//	DEB_IS_CLOSED_WORD_COL,
	DEB_CLOSED_DATE_COL,
	DEB_AMOUNT_COL,
	DEB_PAY_AMOUNT_COL,
	DEB_CURRENCY_ID_COL,
	DEB_REMIND_COL,
	DEB_REMIND_DATE_COL,
	DEB_DESCR_COL
};

enum {
	PLAN_ID_COL,
	PLAN_TIME_T_COL,
	PLAN_DATE_COL,
	PLAN_ACCOUNT_ID_COL,
	PLAN_CATEGORY_ID_COL,
	PLAN_SUBCATEGORY_ID_COL,
	PLAN_QUANTITY_COL,
	PLAN_UNIT_ID_COL,
	PLAN_AMOUNT_COL,
	PLAN_CURRENCY_ID_COL,
	PLAN_PERIODICITY_COL,
	PLAN_PERIODICITY_DAYS_COL,
	PLAN_DESCR_COL
};


ExGrid  *grid_expenditure, 
        *grid_income, 
        *grid_account_short,
        *grid_account_full,
        *grid_debt,
        *grid_credit,
        *grid_plan_income,
        *grid_plan_expenditure;


GtkWidget *main_window;
GtkWidget* create_main_window (void);

void fill_grid_expenditure(void);
void fill_grid_income(void);
void fill_grid_account_short(void);
void fill_grid_account_full(void);
void fill_grid_debt(void);
void fill_grid_credit(void);
void fill_grid_plan_expenditure(void);
void fill_grid_plan_income(void);

void refresh_account_combos();
void refresh_in_category_combos();
void refresh_out_category_combos();
