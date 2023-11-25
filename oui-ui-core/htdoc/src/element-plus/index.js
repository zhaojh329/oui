import ElementPlus from 'element-plus'
import 'element-plus/dist/index.css'
import 'element-plus/theme-chalk/dark/css-vars.css'
import * as ElementPlusIconsVue from '@element-plus/icons-vue'
import zhCN from 'element-plus/dist/locale/zh-cn.mjs'
import zhTW from 'element-plus/dist/locale/zh-tw.mjs'
import jaJP from 'element-plus/dist/locale/ja.mjs'
import enUS from 'element-plus/dist/locale/en.mjs'

import { computed } from 'vue'
import oui from '../oui'

const locales = {
  'en-US': enUS,
  'zh-CN': zhCN,
  'zh-TW': zhTW,
  'ja-JP': jaJP
}

function globalLocale() {
  if (oui.state.locale === 'auto')
    return navigator.language
  else
    return oui.state.locale
}

export const locale = computed(() => locales[globalLocale()])

export default {
  install(app) {
    app.use(ElementPlus)

    for (const [key, component] of Object.entries(ElementPlusIconsVue)) {
      app.component(key, component)
    }
  }
}
