include YARD
include Templates

module MyTemplateHelper
  def linkify(*args)
    if args.first.is_a?(String)
      case args.first
      when /^taghead:(\S+)/
        tag_name = $1
        params = args[1]
        return "<h3 class=\"note\" id=\"#{tag_name}\" toc-title=\"#{tag_name}\"><strong>#{tag_name}:</strong> #{params}</h3>"
      when /^valhead:(\S+)/
        tag_name = $1
        return "<h3 class=\"note private\" id=\"#{tag_name}\" toc-title=\"#{tag_name}\"><strong>#{tag_name}</strong></h3>"
      when /^tag:(\S+)/
        tag_name = $1
        return "<a href=\"##{tag_name}\">#{tag_name}</a>"
      else
          super
      end
    end
    super
  end
end

Template.extra_includes << MyTemplateHelper
Engine.register_template_path(File.dirname(__FILE__))
