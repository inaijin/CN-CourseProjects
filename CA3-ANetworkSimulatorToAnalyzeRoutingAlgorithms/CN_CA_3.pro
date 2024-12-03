TEMPLATE = subdirs

SUBDIRS += src \
           app \
           tests

DISTFILES += \
    .clang-format \
    .clang-tidy \
    .gitignore \
    assets/config.json \
    logs/*

app.depends = src
tests.depends = src
