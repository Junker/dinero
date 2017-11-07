project('dinero', 'c',
	version : '0.1',
	license : 'GPL3',
	default_options : ['warning_level=1'])

##### DEPENDENCIES
gtk_dep = dependency('gtk+-3.0')
gda_dep = dependency('libgda-5.0')
gda_sqlite_dep = dependency('libgda-sqlite-5.0')
gda_ui_dep = dependency('libgda-ui-5.0')

dev = get_option('dev')

if dev
	pkgdatadir = '..'
else
	pkgdatadir = get_option('prefix') + '/' + get_option('datadir') + '/' + meson.project_name()
endif

localedir = get_option('prefix') + '/' + get_option('localedir') 

##### CONFIGURATION
conf_data = configuration_data()
conf_data.set('version', meson.project_version())
conf_data.set('name', meson.project_name())
conf_data.set('data_dir', pkgdatadir) 
conf_data.set('locale_dir', localedir) 
configure_file(
	input : 'config.h.meson', 
	output : 'config.h', 
	configuration : conf_data)

##### LOCALIZATION
subdir('po')


### GLADE DATA
install_subdir('ui', install_dir: pkgdatadir)

### DB
install_data('db/dinero.sqlite', install_dir: pkgdatadir)


##### BUILD
sources = [
		'src/account_datedetailed.h',
		'src/actions.h', 
		'src/backup.h', 
		'src/category.h',
		'src/common.h',
		'src/currency.h',
		'src/db.h',
		'src/editaccount.h',
		'src/debtcredit_payment.h',
		'src/editexchange.h',
		'src/editdebt.h',
		'src/editexpin.h',
		'src/editplan.h',
		'src/edittransfer.h',
		'src/ex-connection.h',
		'src/ex-grid.h',
		'src/exchange.h',
		'src/export.h',
		'src/gtkdateentry.h',
		'src/plan.h',
		'src/reminddebt.h',
		'src/report.h',
		'src/sutil.h',
		'src/transfer.h',
		'src/mainform.h',
		'src/account_datedetailed.c',
		'src/actions.c', 
		'src/category.c',
		'src/backup.c',
		'src/common.c',
		'src/currency.c',
		'src/db.c',
		'src/debtcredit_payment.c',
		'src/editaccount.c',
		'src/editdebt.c',
		'src/editexchange.c',
		'src/editexpin.c',
		'src/editplan.c',
		'src/edittransfer.c',
		'src/ex-connection.c',
		'src/ex-grid.c',
		'src/exchange.c',
		'src/export.c',
		'src/gtkdateentry.c',
		'src/plan.c',
		'src/reminddebt.c',
		'src/report.c',
		'src/sutil.c',
		'src/transfer.c',
		'src/mainform.c',
]

executable('dinero', 'src/main.c',
	sources,
	dependencies : [gtk_dep, gda_dep, gda_sqlite_dep, gda_ui_dep],
	install: true,
	link_args : ['-rdynamic']
)