<template>
  <el-button v-if="visible" style="margin-top: 10px" type="primary" size="mini" @click="handleAdd">+ {{ $t('Add') }}</el-button>
</template>

<script>
export default {
  name: 'UciSectionAdd',
  inject: ['uciForm'],
  props: {
    sestion: Object
  },
  computed: {
    visible() {
      const s = this.sestion;
      return s.addable && s.type && !s.name;
    },
    prop() {
      return 'uci_sestion_' + this.sestion.type;
    }
  },
  data() {
    return {
      nameVisible: false,
      rules: [
        {validator: this.validateName}
      ],
      valid: false
    }
  },
  methods: {
    add(name) {
      let sid;

      if (this.sestion.add)
        sid = this.sestion.add(this.sestion, name);
      else
        sid = this.$uci.add(this.sestion.config, this.sestion.type, name);

      if (sid) {
        this.uciForm.activeCollapseItem = sid;
        this.sestion.load();
        this.sestion.buildForm(sid);
      }
    },
    handleAdd() {
      if (this.sestion.anonymous) {
        this.add();
        return;
      }

      this.$prompt(this.$t('Please input a name'), this.$t('Add'), {
        inputValidator: value => {
          if (!value || value.match(/^[a-zA-Z0-9_]+$/) === null)
            return this.$t('Must be a valid UCI identifier');

          for (let i = 0; i < this.sestion.sids.length; i++)
            if (this.sestion.sids[i] === value)
              return this.$t('Name already used');

          return true;
        }
      }).then(r => {
        this.add(r.value);
      });
    }
  }
}
</script>
