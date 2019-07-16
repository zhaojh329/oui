<template>
  <div class="oui-uci-dlist">
    <el-tag :closable="!readonly" v-for="tag in value" :key="tag" :disable-transitions="false" @close="handleDelDlist(tag)">{{ tag }}</el-tag>
    <template v-if="inputVisible">
      <el-autocomplete v-if="hasSuggestions" size="small" ref="input" v-model="inputValue" @keyup.enter.native="handleinputConfirm" :fetch-suggestions="fetchSuggestions" @blur="handleinputConfirm"></el-autocomplete>
      <el-input v-else size="small" ref="input" v-model="inputValue" @keyup.enter.native="handleinputConfirm" @blur="handleinputConfirm"></el-input>
    </template>
    <el-button v-else size="mini" type="primary" plain @click="showInput">+ {{ $t('Add') }}</el-button>
  </div>
</template>

<script>
export default {
  name: 'UciDlist',
  inject: ['uciForm'],
  props: {
    value: {
      type: Array,
      default: () => {
        return [];
      }
    },
    prop: {
      type: String,
      required: true
    },
    suggestions: Array
  },
  data() {
    return {
      inputVisible: false,
      inputValue: ''
    }
  },
  computed: {
    readonly() {
      return this.uciForm.readonly;
    },
    form() {
      return this.uciForm.form;
    },
    hasSuggestions() {
      return this.suggestions && this.suggestions.length > 0;
    }
  },
  methods: {
    fetchSuggestions(queryString, cb) {
      const suggestions = this.suggestions;
      const results = queryString ? suggestions.filter(suggestion => {
        return (suggestion.toLowerCase().indexOf(queryString.toLowerCase()) === 0);
      }) : suggestions;
      cb(results.map(r => {
        return {value: r};
      }));
    },
    handleDelDlist(tag) {
      let tags = [...this.value];
      tags.splice(tags.indexOf(tag), 1);
      this.$emit('input', tags);
    },
    showInput() {
      this.inputVisible = true;
      this.$nextTick(() => {
        this.$refs['input'].focus();
      });
    },
    clear() {
      this.inputVisible = false;
      this.inputValue = '';
    },
    handleinputConfirm() {
      setTimeout(() => {
        this.__handleinputConfirm();
      }, 100);
    },
    __handleinputConfirm() {
      if (this.inputValue) {
        const tags = [...this.value];

        if (tags.indexOf(this.inputValue) > -1) {
          this.clear();
          return;
        }

        tags.push(this.inputValue);
        this.$emit('input', tags);

        this.$nextTick(() => {
          this.uciForm.$refs['form'].validateField(this.prop, (err) => {
            if (err) {
              this.form[this.prop].splice(this.form[this.prop].indexOf(this.inputValue), 1);
            } else {
              this.clear();
            }
          })
        });
      } else {
        this.inputVisible = false;
      }
    }
  }
}
</script>

<style lang="scss">
.oui-uci-dlist {
  display: flex;
  flex-direction: column;

  > * {
    margin-bottom: 5px;
  }

  .el-button {
    width: 100px;
  }
}
</style>
