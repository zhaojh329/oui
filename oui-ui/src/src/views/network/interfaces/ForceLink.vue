<template>
  <oui-form-item-switch :uci-section="uciSection" :label="$t('Force link')" name="force_link" :initial="forceLinkInitial" :help="$t('Set interface properties regardless of the link carrier (If set, carrier sense events do not invoke hotplug handlers).')"/>
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
