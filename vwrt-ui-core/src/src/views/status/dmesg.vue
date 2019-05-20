<template>
  <Input v-model="dmesg" type="textarea" :autosize="true" />
</template>

<script>

export default {
  name: 'dmesg',
  data() {
    return {
      dmesg: ''
    }
  },
  mounted() {
    this.$ubus.call('vwrt.system', 'dmesg').then(r => {
      this.dmesg = r.log;
    });
  }
}
</script>
