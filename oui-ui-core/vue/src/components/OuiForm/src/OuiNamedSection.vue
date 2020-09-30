<template>
  <component :is="container" :title="title">
    <slot v-if="section"  v-bind:s="section"/>
  </component>
</template>

<script>
import OuiSection from './OuiSection.vue'

export default {
  name: 'OuiNamedSection',
  mixins: [OuiSection],
  props: {
    name: {
      type: String,
      required: true
    }
  },
  computed: {
    section () {
      const sections = this.$uci.sections(this.uciConfig).filter(s => s['.name'] === this.name)
      return sections.length > 0 ? sections[0] : null
    }
  }
}
</script>
