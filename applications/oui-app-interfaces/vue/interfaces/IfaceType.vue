<template>
  <oui-form-item-switch :uci-section="uciSection" :label="$t('interfaces.Bridge interfaces')" name="type" true-value="bridge" :save="saveType" @change="changed" :help="$t('interfaces.creates a bridge over specified interface(s)')"/>
</template>

<script>
export default {
  name: 'IfaceTyoe',
  props: {
    uciSection: Object
  },
  methods: {
    saveType (self) {
      self.uci.set(self.sid, 'type', self.model || '')
    },
    changed (self) {
      this.$emit('change', self)
    }
  }
}
</script>
