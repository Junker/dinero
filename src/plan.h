/*
 * plan_payment.h
 *
 * Copyright (C) 2011 - Dmitry Kosenkov
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


// Periodicity days flags
#define PERIODICITY_MONDAY    (1 << 0) 
#define PERIODICITY_TUESDAY   (1 << 1) 
#define PERIODICITY_WEDNESDAY (1 << 2) 
#define PERIODICITY_THURSDAY  (1 << 3) 
#define PERIODICITY_FRIDAY    (1 << 4) 
#define PERIODICITY_SATURDAY  (1 << 5) 
#define PERIODICITY_SUNDAY    (1 << 6)

enum {
	PERIODICITY_ONCE,
	PERIODICITY_DAY,
	PERIODICITY_MONTH,
	PERIODICITY_QUARTER,
	PERIODICITY_YEAR
};

void show_plan_payment_window ();