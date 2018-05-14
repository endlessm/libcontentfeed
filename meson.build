# Copyright 2018 Endless Mobile, Inc.

project('libcontentfeed', 'c', version: '0.1.0', license: 'LGPL2.1+',
    meson_version: '>= 0.42.0',
    default_options: ['c_std=c99', 'warning_level=3'])

libtool_version = '0.0.0'

gnome = import('gnome')
i18n = import('i18n')
pkg = import('pkgconfig')

version_components = meson.project_version().split('.')
api_version = version_components[0]
api_name = '@0@-@1@'.format(meson.project_name(), api_version)
namespace_name = 'EosDiscoveryFeed'

# Dependencies

eos_shard = dependency('eos-shard-0')
gio = dependency('gio-2.0')
gio_unix = dependency('gio-unix-2.0')
glib = dependency('glib-2.0')
gobject = dependency('gobject-2.0')
libsoup = dependency('libsoup-2.4')
m_dep = meson.get_compiler('c').find_library('m', required : false)

subdir('src')

requires = ['eos-shard-0', 'glib-2.0', 'gio-2.0', 'gio-unix-2.0', 'gobject-2.0', 'libsoup-2.4']
pkg.generate(filebase: api_name, libraries: [main_library],
    description: 'Content Feed generation library for Discovery Feed and Companion App.',
    name: meson.project_name(), subdirs: api_name, requires: requires,
    url: 'http://endlessm.github.io/libcontentfeed',
    version: meson.project_version())

# Summary

message('\n'.join([
    '@0@ @1@ (@2@)'.format(meson.project_name(), meson.project_version(),
        api_name),
    '-------------------',
    'Directories:',
    '    Install prefix: @0@'.format(get_option('prefix')),
    ''
]))
