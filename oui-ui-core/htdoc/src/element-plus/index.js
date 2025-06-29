import ElementPlus from 'element-plus'
import 'element-plus/dist/index.css'
import 'element-plus/theme-chalk/dark/css-vars.css'
import * as ElementPlusIconsVue from '@element-plus/icons-vue'
import zhCN from 'element-plus/es/locale/lang/zh-cn'
import zhTW from 'element-plus/es/locale/lang/zh-tw'
import jaJP from 'element-plus/es/locale/lang/ja'
import en from 'element-plus/es/locale/lang/en'

import { computed } from 'vue'
import oui from '../oui'

const locales = {
  'en': en,
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
