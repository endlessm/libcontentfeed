# Copyright 2018 Endless Mobile, Inc.

version = configuration_data()
version.set('EOS_DISCOVERY_FEED_MAJOR_VERSION', version_components[0])
version.set('EOS_DISCOVERY_FEED_MINOR_VERSION', version_components[1])
version.set('EOS_DISCOVERY_FEED_MICRO_VERSION', version_components[2])
version_h = configure_file(configuration: version,
    input: 'libcontentfeed-version.h.in', output: 'libcontentfeed-version.h')

# Generated Configuration File

config = configuration_data()
config.set_quoted('LIBCONTENTFEED_VERSION', meson.project_version())
config.set_quoted('GETTEXT_PACKAGE', 'libcontentfeed')
config.set_quoted('PACKAGE_LOCALE_DIR',
                  join_paths(get_option('prefix'),
                             get_option('datadir'),
                             'locale'))
configure_file(configuration: config, output: 'config.h')

enum_headers = [
    'feed-base-card-store.h',
    'feed-card-layout-direction.h',
    'feed-sizes.h'
]

installed_headers = [
    'libcontentfeed.h',
    version_h,
    enum_headers,
    'feed-app-card-store.h',
    'feed-knowledge-app-artwork-card-store.h',
    'feed-knowledge-app-card-store.h',
    'feed-knowledge-app-news-card-store.h',
    'feed-knowledge-app-proxy.h',
    'feed-knowledge-app-video-card-store.h',
    'feed-model-ordering.h',
    'feed-orderable-model.h',
    'feed-provider-info.h',
    'feed-provider-lookup.h',
    'feed-proxy-factory.h',
    'feed-quote-card-store.h',
    'feed-store-provider.h',
    'feed-text-sanitization.h',
    'feed-word-card-store.h',
    'feed-word-quote-card-store.h'
]
sources = [
    'feed-all-async-tasks.c',
    'feed-app-card-store.c',
    'feed-base-card-store.c',
    'feed-knowledge-app-artwork-card-store.c',
    'feed-knowledge-app-card-store.c',
    'feed-knowledge-app-news-card-store.c',
    'feed-knowledge-app-proxy.c',
    'feed-knowledge-app-video-card-store.c',
    'feed-model-ordering.c',
    'feed-orderable-model.c',
    'feed-provider-info.c',
    'feed-provider-lookup.c',
    'feed-proxy-factory.c',
    'feed-quote-card-store.c',
    'feed-store-provider.c',
    'feed-text-sanitization.c',
    'feed-word-card-store.c',
    'feed-word-quote-card-store.c'
]

enum_sources = gnome.mkenums_simple('feed-enums', install_header: true,
    install_dir: join_paths(get_option('includedir'), api_name),
    sources: enum_headers)

main_library = library('@0@-@1@'.format(meson.project_name(), api_version),
    enum_sources, sources, installed_headers,
    c_args: ['-DG_LOG_DOMAIN="@0@"'.format(namespace_name), '-DCOMPILING_LIBCONTENTFEED'],
    dependencies: [eos_shard, gio, gio_unix, glib, gobject, libsoup, m_dep],
    install: true,
    link_depends: '@0@.map'.format(meson.project_name()),
    soversion: api_version, version: libtool_version)

introspection_sources = [sources, enum_sources, installed_headers]

gnome.generate_gir(
    main_library,
    extra_args: [
        '--warn-all',
        '--warn-error',
        '-DG_LOG_DOMAIN="@0@"'.format(namespace_name),
        '-DCOMPILING_LIBCONTENTFEED'
    ],
    identifier_prefix: 'EosDiscoveryFeed',
    includes: ['Gio-2.0', 'GLib-2.0', 'GObject-2.0'],
    install: true, namespace: namespace_name, nsversion: api_version,
    sources: introspection_sources, symbol_prefix: 'eos_discovery_feed',
)

install_headers(installed_headers, subdir: api_name)
