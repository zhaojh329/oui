<template>
   <oui-form uci-config="rtty">
    <oui-typed-section type="rtty" addremove :collabsible="false" v-slot="{ s }">
      <oui-form-item-select :uci-section="s" :label="$t('rtty.Interface')" name="interface" :options="interfaces"/>
      <oui-form-item-input :uci-section="s" label="ID" name="id"/>
      <oui-form-item-input :uci-section="s" :label="$t('rtty.Description')" name="description"/>
      <oui-form-item-input :uci-section="s" :label="$t('rtty.Host')" name="host" required/>
      <oui-form-item-input :uci-section="s" :label="$t('rtty.Port')" name="port" placeholder="5912" rules="port"/>
      <oui-form-item-switch :uci-section="s" label="SSL" name="ssl"/>
      <oui-form-item-input :uci-section="s" :label="$t('rtty.Token')" name="token"/>
    </oui-typed-section>
  </oui-form>
</template>

<script>
export default {
  data () {
    return {
      interfaces: []
    }
  },
  created () {
    this.$network.load().then(() => {
      const interfaces = this.$network.getInterfaces()
      this.interfaces.push(...interfaces.map(item => item.name))
    })
  }
}
</script>
