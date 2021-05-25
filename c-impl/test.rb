#/usr/bin/env mruby

$target = "./db"

def echo(cmd)
    f = IO.popen("echo '#{cmd}' | #{$target}")
    return f.readlines
end

def test()
    res1 = echo(".test")
    # puts res1

    # res1.each do |msg|
    #     puts msg
    # end

    if res1[0] == "db> Unrecoginzed command '.test'. \n" and
        res1[1] == "db> Error reading input\n"
        puts "OK!"
    end

    res2 = echo(".exit")
    # puts res2

    # res2.each do |msg|
    #     puts msg
    # end

    if res2[0] == "db> "
        puts "OK!!"
    end
end

test()