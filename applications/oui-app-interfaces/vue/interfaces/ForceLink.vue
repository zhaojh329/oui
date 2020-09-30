<template>
  <oui-form-item-switch :uci-section="uciSection" :label="$t('interfaces.Force link')" name="force_link" :initial="forceLinkInitial" :help="$t('interfaces.ForceLinkHelp')"/>
</template>

<script>
export default {
  name: 'ForceLink',
  props: {
    uciSection: Object
  },
  computed: {
    forceLinkInitial () {
      return this.uciSection.proto === 'static'
    }
  }
}
</script>
