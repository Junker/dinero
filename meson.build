project('dinero', 'c',
	version : '0.8',
	license : 'GPL3',
	default_options : ['warning_level=3'])

##### DEPENDENCIES
gtk_dep = dependency('gtk+-3.0')
gda_dep = dependency('libgda-5.0')
gda_sqlite_dep = dependency('libgda-sqlite-5.0')
gda_ui_dep = dependency('libgda-ui-5.0')


pkgdatadir = get_option('prefix') + '/' + get_option('datadir') + '/' + meson.project_name()
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

##### BUILD
sources = [
		'src/account_datedetailed.c',
		'src/account_datedetailed.h',
		'src/actions.c', 
		'src/actions.h', 
		'src/category.c', 
		'src/category.h',
		'src/common.c',
		'src/common.h',
		'src/currency.c',
		'src/currency.h',
		'src/db.c',
		'src/db.h',
		'src/debtcredit_payment.c',
		'src/debtcredit_payment.h',
		'src/editaccount.c',
		'src/editaccount.h',
		'src/editdebt.c',
		'src/editdebt.h',
		'src/editexchange.c',
		'src/editexchange.h',
		'src/editexpin.c',
		'src/editexpin.h',
		'src/editplan.c',
		'src/editplan.h',
		'src/edittransfer.c',
		'src/edittransfer.h',
		'src/ex-connection.c',
		'src/ex-connection.h',
		'src/ex-grid.c',
		'src/ex-grid.h',
		'src/gtkdateentry.c',
		'src/gtkdateentry.h',
		'src/mainform.c',
		'src/mainform.h',
		'src/plan.c',
		'src/plan.h',
		'src/reminddebt.c',
		'src/reminddebt.h',
		'src/report.c',
		'src/report.h',
		'src/sutil.c',
		'src/sutil.h',
		'src/transfer.c',
		'src/transfer.h']

executable('dinero', 'src/main.c',
	sources,
	dependencies : [gtk_dep, gda_dep, gda_sqlite_dep, gda_ui_dep],
	link_args : ['-rdynamic']
)