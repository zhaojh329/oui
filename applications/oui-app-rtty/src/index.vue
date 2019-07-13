<template>
   <uci-form config="rtty">
    <uci-section type="rtty" addable :collabsible="false">
      <uci-option-list :label="$t('Interface')" name="interface" :options="interfaces"></uci-option-list>
      <uci-option-input label="ID" name="id"></uci-option-input>
      <uci-option-input :label="$t('Description')" name="description"></uci-option-input>
      <uci-option-input :label="$t('Host')" name="host" required></uci-option-input>
      <uci-option-input :label="$t('Port')" name="port" placeholder="5912" rules="port"></uci-option-input>
      <uci-option-switch label="SSL" name="ssl"></uci-option-switch>
      <uci-option-input :label="$t('Keepalive Time')" name="keepalive" placeholder="5" rules="uinteger" append="s"></uci-option-input>
      <uci-option-input :label="$t('Token')" name="token"></uci-option-input>
    </uci-section>
  </uci-form>
</template>

<script>
export default {
  data() {
    return {
      interfaces: []
    }
  },
  created() {
    this.$network.load().then(() => {
      const interfaces = this.$network.getInterfaces();
      this.interfaces.push(...interfaces.map(item => item.name));
    });
  }
}
</script>
