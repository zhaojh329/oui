<template>
  <Input v-model="syslog" type="textarea" :autosize="true" />
</template>

<script>

export default {
  name: 'syslog',
  data() {
    return {
      syslog: ''
    }
  },
  mounted()  {
    this.$ubus.call('vwrt.system', 'syslog').then(r => {
      this.syslog = r.log;
    });
  }
}
</script>
