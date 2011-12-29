
## Process this file with automake to produce Makefile.in

## Created by Anjuta

sharedir = $(datadir)/homefinances
share_DATA = \
        homefinances.db

gladedir = $(datadir)/homefinances/glade
glade_DATA = \
	ui/account_datedetailed.glade\
	ui/category.glade \
	ui/currency.glade \
	ui/editaccount.glade \
	ui/editdebt.glade \
	ui/editexchange.glade \
	ui/editexpin.glade \
	ui/edittransfer.glade \
	ui/inputdialog.glade \
	ui/mainform.glade \
	ui/clock_edit.png \
	ui/coins.png \
	ui/group.png \
	ui/money_add.png \
	ui/money_delete.png \
	ui/menu_categories24.png \
	ui/menu_categories48.png \
	ui/_menu_currency48.png \
	ui/menu_currency24.png \
	ui/menu_currency48.png \
	ui/menu_report24.png \
	ui/menu_report48.png \
	ui/menu_transfer24.png \
	ui/menu_transfer48.png \
	ui/gridform.glade \
	ui/reminddebt.glade \
	ui/editdebtcredit_payment.glade \
	ui/full.png \
	ui/short.png \
	ui/angel.png \
	ui/devil.png \
	ui/filter.png \
	ui/report.glade


AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	$(HOMEFINANCE_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

bin_PROGRAMS = homefinances

homefinances_SOURCES = \
	main.c \
	db.c \
	db.h \
	currency.c \
	currency.h \
	mainform.c \
	mainform.h \
	actions.c \
	actions.h \
	category.c \
	category.h \
	editdebt.c \
	editdebt.h \
	edittransfer.c \
	edittransfer.h \
	exchange.c \
	exchange.h \
	account_datedetailed.c \
	account_datedetailed.h \
	transfer.c \
	transfer.h \
	editexchange.c \
	editexchange.h \
	report.c \
	report.h \
	main.h \
	ex-grid.h \
	ex-grid.c \
	sutil.c \
	sutil.h \
	ex-connection.h \
	ex-connection.c \
	editexpin.c \
	editexpin.h \
	editaccount.c \
	editaccount.h \
	debtcredit_payment.c \
	debtcredit_payment.h \
	editplan.c \
	editplan.h \
	plan.c \
	plan.h \
	reminddebt.c \
	reminddebt.h \
	gtkdateentry.c \
	gtkdateentry.h

homefinances_LDFLAGS = \
	-Wl,--export-dynamic

homefinances_LDADD = $(HOMEFINANCE_LIBS)

EXTRA_DIST = $(glade_DATA) \
	$(pixmaps_DATA) \
        $(share_DATA)

# Remove glade directory on uninstall
uninstall-local:
	-rm -r $(gladedir)
##	-rm -r $(datadir)/homefinances

