<template>
  <n-config-provider :theme="theme" :locale="locale" :date-locale="dateLocale" inline-theme-disabled>
    <router-view/>
  </n-config-provider>
</template>

<script>
import {
  darkTheme,
  zhCN, zhTW,
  dateZhCN, dateZhTW,
  jaJP, dateJaJP
} from 'naive-ui'

const locales = {
  'zh-CN': [zhCN, dateZhCN],
  'zh-TW': [zhTW, dateZhTW],
  'ja-JP': [jaJP, dateJaJP]
}

export default {
  computed: {
    globalLocale() {
      if (this.$oui.state.locale === 'auto')
        return navigator.language
      else
        return this.$oui.state.locale
    },
    theme() {
      return this.$oui.state.theme === 'dark' ? darkTheme : undefined
    },
    locale() {
      return locales[this.globalLocale]?.[0]
    },
    dateLocale() {
      return locales[this.globalLocale]?.[1]
    }
  }
}
</script>
