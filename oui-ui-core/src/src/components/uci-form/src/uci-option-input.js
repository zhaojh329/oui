import mixin from './uci-option'

export default {
  name: 'UciOptionInput',
  mixins: [mixin],
  props: {
    placeholder: String,
    password: Boolean,
    append: String,
    suggestions: Array,
    minlength: Number,
    maxlength: Number
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
    view(prop) {
      if (this.suggestions && this.suggestions.length > 0)
        return (
          <el-autocomplete fetch-suggestions={this.fetchSuggestions} placeholder={this.placeholder} minlength={this.minlength} maxlength={this.maxlength} v-model={this.form[prop]}>
            { this.append && <span slot="append">{ this.append }</span> }
          </el-autocomplete>
        );
      else
        return (
          <el-input placeholder={this.placeholder} show-password={this.password} minlength={this.minlength} maxlength={this.maxlength} v-model={this.form[prop]}>
            { this.append && <span slot="append">{ this.append }</span> }
          </el-input>
        );
    }
  }
}
