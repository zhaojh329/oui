#!/usr/bin/env sh

# abort on errors
set -e

# build
vuepress build .

# navigate into the build output directory
cd .vuepress/dist

git init
git add -A
git commit -m 'deploy'

git push -f https://github.com/zhaojh329/oui.git master:gh-pages
git push -f https://gitee.com/zhaojh329/oui.git master:gh-pages

cd -
