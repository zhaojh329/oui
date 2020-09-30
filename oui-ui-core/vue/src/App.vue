<template>
  <a-config-provider id="app" :locale="locale">
    <fullscreen ref="fullscreen">
      <a-spin v-if="loaded" :spinning="spinning > 0" :tip="spintip || $t('Loading...')" size="large">
        <router-view></router-view>
      </a-spin>
    </fullscreen>
  </a-config-provider>
</template>

<script>
import zhCN from 'ant-design-vue/es/locale-provider/zh_CN'
import zhTW from 'ant-design-vue/es/locale-provider/zh_TW'
import jaJP from 'ant-design-vue/es/locale-provider/ja_JP'
import { mapState } from 'vuex'

export default {
  data () {
    return {
      loaded: false,
      locales: {
        'zh-cn': zhCN,
        'zh-tw': zhTW,
        ja: jaJP
      }
    }
  },
  computed: {
    locale () {
      return this.locales[this.$i18n.locale]
    },
    ...mapState(['fullscreen', 'spinning', 'spintip'])
  },
  watch: {
    fullscreen () {
      this.$refs.fullscreen.toggle()
    }
  },
  created () {
    this.$rpc.call('oui', 'get_lang').then(({ lang }) => {
      this.$store.commit('setLang', lang)
      if (lang === 'auto') lang = navigator.language.toLowerCase()

      if (lang === 'zh') lang = 'zh-cn'

      this.$rpc.call('oui', 'load_locales', { locale: lang }).then(locales => {
        locales.forEach(locale => this.$i18n.mergeLocaleMessage(lang, locale))
        this.$i18n.locale = lang
        this.loaded = true
      })
    })
  }
}
</script>
