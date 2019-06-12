<template>
  <div>
    <CardList :title="$t('Device information')" :list="devinfo"></CardList>
  </div>
</template>

<script>

export default {
  data() {
    return {
      devinfo: []
    }
  },
  created() {
    this.$system.getBoardInfo().then(r => {
      this.devinfo = [
        [this.$t('Model'), r.model],
        [this.$t('Architecture'), r.system],
        [this.$t('Kernel Version'), r.kernel],
        [this.$t('Firmware Version'), r.release.revision]
      ];
    });
  }
}
</script>
