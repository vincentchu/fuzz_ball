module FuzzBall
  class DupleIndex

    # def match(needle, opts = {})

		#   @cache ||= {}

    #   return {} if needle.length < 2

    #   matches = recursive_match(needle)

    #   by_scores = {}
    #   matches.keys.each do |str_index|
    #     count = matches[str_index][:count]

    #     by_scores[count] ||= []
    #     by_scores[count] << str_index

    #     matches[str_index] = count
    #   end

    #   opts[:by_score] ? by_scores : matches
    # end

    private

    def recursive_match(needle)

      if (needle.length == 2)
        return self.query(*needle).inject({}) do |hsh, m|
          hsh.merge(m[0] => {:count => 1, :position => m[1]})
        end
      else
				
				key = needle.join(",")
				return @cache[key] if @cache[key]

        last_char  = needle.pop
        last_count = recursive_match(needle)

        matches    = query(needle.last, last_char)

				# puts "matches = #{matches.inspect}"
				# puts "last_char = #{last_char}, #{needle.last}, #{needle.inspect}"
				# puts "last_count = #{last_count.inspect}"

        matches.each do |match|
					# puts "match = #{match.inspect}"
					# puts "last_count[match[0]] = #{last_count[match[0]].inspect}"


          if (last_count[match[0]] && last_count[match[0]][:position] < match[1])
            last_count[match[0]][:count]    += 1
            last_count[match[0]][:position]  = match[1]
					else
						last_count[match[0]] = {:count => 1, :position => match[1]}
          end
        end

				@cache[key] = last_count

        return last_count
      end
    end
  end
end
